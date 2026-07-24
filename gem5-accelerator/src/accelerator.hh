#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <vector>
#include <iomanip>
#include "memory_map.hh"

// b_transport on cfg_socket blocks until the full system concludes:
//  DMA read RGB -> convert -> DMA write grayscale
struct Accelerator : sc_core::sc_module {

    tlm_utils::simple_target_socket<Accelerator>    cfg_socket;
    tlm_utils::simple_initiator_socket<Accelerator> dma_socket;

    SC_CTOR(Accelerator) : cfg_socket("cfg_socket"), dma_socket("dma_socket") {
        cfg_socket.register_b_transport(this, &Accelerator::on_config);
    }

private:
    
    uint64_t _input_addr;
    uint64_t _output_addr;
    uint32_t _num_pixels;
    uint32_t _command = 0;
    uint32_t _status = STATUS_IDLE;

    template<typename T>
    bool copy_register_to_transaction(
            const T& value,
            uint8_t* data,
            unsigned int length)
    {
        if (length != sizeof(T)) {
            _status = STATUS_ERROR;
            return false;
        }

        std::memcpy(data, &value, sizeof(T));
        return true;
    }

    template<typename T>
    bool copy_transaction_to_register(
                T& value,
                const uint8_t* data,
                unsigned int length)
    {
        if (length != sizeof(T)) {
            _status = STATUS_ERROR;
            return false;
        }

        std::memcpy(&value, data, sizeof(T));
        return true;
    }
    
    void start_accelerator(sc_core::sc_time& delay)
    {
        if (_status == STATUS_BUSY) {
            _status = STATUS_ERROR;
            SC_REPORT_ERROR("Accelerator",
                            "Already busy");
            return;
        }

        if (_num_pixels == 0) {
            _status = STATUS_ERROR;
            SC_REPORT_ERROR("Accelerator",
                "Num pixels not set or is size 0");
            return;
        }
        
        const uint64_t rgb_bytes_64 =
        static_cast<uint64_t>(_num_pixels) * 3;

        if (rgb_bytes_64 > std::numeric_limits<uint32_t>::max()) {
            _status = STATUS_ERROR;
            return;
        }

        _status = STATUS_BUSY;

        const uint32_t rgb_bytes =  static_cast<uint32_t>(rgb_bytes_64);

        std::vector<uint8_t> rgb(rgb_bytes);

        SC_REPORT_INFO("Accel", "Reading RGB data");
        if (!dma_transfer(tlm::TLM_READ_COMMAND, _input_addr,
                        rgb.data(), rgb_bytes, delay)) {
            _status = STATUS_ERROR;
            return;
        }

        std::vector<uint8_t> gray(_num_pixels);

         SC_REPORT_INFO("Accel", "Converting..");
        for (uint32_t i = 0; i < _num_pixels; ++i) {
            gray[i] = static_cast<uint8_t>(
                (
                    77u  * rgb[i * 3] +
                    150u * rgb[i * 3 + 1] +
                    29u  * rgb[i * 3 + 2]
                ) >> 8
            );
        }

        delay += sc_core::sc_time(
            static_cast<double>(_num_pixels) *
            ACCEL_NS_PER_PIXEL, sc_core::SC_NS);

        SC_REPORT_INFO("Accel", "Writing gray data");
        if (!dma_transfer(tlm::TLM_WRITE_COMMAND, _output_addr, 
            gray.data(), _num_pixels, delay)) {
            _status = STATUS_ERROR;
            return;
        }

        _status = STATUS_DONE;
    }

    void handle_register_read( uint64_t offset, 
                                uint8_t* data,
                                unsigned int length)
    {
        switch (offset) {
            case REG_INPUT_ADDR:
                copy_register_to_transaction(
                    _input_addr, data, length);
                break;

            case REG_OUTPUT_ADDR:
                copy_register_to_transaction(
                    _output_addr, data, length);
                break;

            case REG_NUM_PIXELS:
                copy_register_to_transaction(
                    _num_pixels, data, length);
                break;

            case REG_COMMAND:
                copy_register_to_transaction(
                    _command, data, length);
                break;

            case REG_STATUS:
                copy_register_to_transaction(
                    _status, data, length);
                break;

            default:
                _status = STATUS_ERROR;
                break;
        }
    }

    void handle_register_write(uint64_t offset,
                            const uint8_t* data,
                            unsigned int length,
                            sc_core::sc_time& delay)
    {
        switch (offset) {
            case REG_INPUT_ADDR:
                copy_transaction_to_register(
                    _input_addr, data, length);
                break;

            case REG_OUTPUT_ADDR:
                copy_transaction_to_register(
                    _output_addr, data, length);
                break;

            case REG_NUM_PIXELS:
                copy_transaction_to_register(
                    _num_pixels, data, length);
                break;

            case REG_COMMAND: {
                if (!copy_transaction_to_register(
                        _command, data, length)) {
                    return;
                }
                if (_command == CMD_START) {
                    start_accelerator(delay);
                } else {
                    _status = STATUS_ERROR;
                }
                break;
            }

            case REG_STATUS:
                // Status is read-only.
                _status = STATUS_ERROR;
                break;

            default:
                _status = STATUS_ERROR;
                break;
        }
    }

    bool do_dma(tlm::tlm_command cmd, uint64_t addr,
                uint8_t* data, uint32_t len, sc_core::sc_time& delay)
    {
        tlm::tlm_generic_payload trans;
        trans.set_command(cmd);
        trans.set_address(addr);
        trans.set_data_ptr(data);
        trans.set_data_length(len);
        trans.set_streaming_width(len);
        trans.set_byte_enable_ptr(nullptr);
        trans.set_dmi_allowed(false);
        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);

        dma_socket->b_transport(trans, delay);

        if (trans.is_response_error()) {
            _status = STATUS_ERROR;
            SC_REPORT_ERROR("Accelerator",
                            trans.get_response_string().c_str());
            return false;
        }

        return true;
    }

    bool dma_transfer(tlm::tlm_command command,
                    uint64_t address,
                    uint8_t* data,
                    uint32_t length,
                    sc_core::sc_time& delay)
    {
        uint32_t completed = 0;

        while (completed < length) {
            const uint32_t chunk =
                std::min(DMA_CHUNK, length - completed);

            if (!do_dma(command, address + completed,
                data + completed, chunk, delay)) {
                return false;
            }
            completed += chunk;
        }

        return true;
    }

    void on_config(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {

        const tlm::tlm_command command = trans.get_command();
        const uint64_t address = trans.get_address();
        uint8_t* const data = trans.get_data_ptr();
        const unsigned int length = trans.get_data_length();

        trans.set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        
        if (address < ACCEL_BASE) {
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            SC_REPORT_INFO("Accel", "Address is lower than accel base");
            return;
        }

        if (data == nullptr) {
            trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            SC_REPORT_INFO("Accel", "NULL pointer for get_data");
            return;
        }

        if (trans.get_byte_enable_ptr() != nullptr) {
            trans.set_response_status( tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            SC_REPORT_INFO("Accel", "Non NULL pointer for get_byte_enable");
            return;
        }

        const uint64_t offset = address - ACCEL_BASE;

         if (offset >= ACCEL_SIZE || length > ACCEL_SIZE - offset) {
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            SC_REPORT_INFO("Accel", "Data addr offset is outside range");
            return;
         }


        if (command == tlm::TLM_WRITE_COMMAND) {
            handle_register_write(offset, data, length, delay);
        } else if (command == tlm::TLM_READ_COMMAND) {
            handle_register_read(offset, data, length);
        } else {
            trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
            SC_REPORT_INFO("Accel", "Command not recognized");
        return;
        }

        if (_status == STATUS_ERROR &&
            offset == REG_COMMAND) {
            trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            return;
        }

        delay += sc_core::sc_time(1, sc_core::SC_NS);
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }
};