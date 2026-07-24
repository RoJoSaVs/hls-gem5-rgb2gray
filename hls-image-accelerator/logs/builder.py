# 2026-07-23T18:21:17.369278
import vitis

client = vitis.create_client()
client.set_workspace(path="/home/ronny/tec/high-level-design-for-electronics/homework/homework-3/hls-gem5-rgb2gray/hls-image-accelerator")

comp = client.get_component(name="build")
comp.run(operation="C_SIMULATION")

comp.run(operation="SYNTHESIS")

comp.run(operation="CO_SIMULATION")

client.delete_component(name="build")

client.delete_component(name="build")

