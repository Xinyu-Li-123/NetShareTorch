import random
import netshare.ray as ray
from netshare import Generator
import os 

"""
TODO: Find out what config option can speed up / slow down training

low gpu usage for caida, all sample len can be run on 1 gpu in parallel

batch size: 100
gen: 1 layer, 100 unit

sample_len - time per epoch
    1: 15min
    5: 3min
    10: 5min
    25: 5min
    50: 7min
    100: 12min

batch size: 100
gen: 1 layer, 512 unit
sample_len - time per epoch
    5: 1h
"""

if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    # generator = Generator(config="config_caida_small_nodp.json")
    generator = Generator(config="config_dummy.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    generator.train(work_folder='../../results/caida_c0_1-100_relu_act', preprocess=False, train=True)
    generator.generate(work_folder='../../results/caida_c0_1-100_relu_act')
    # generator.visualize(work_folder='../../results/caida_c0_1-100', port=8056)
    # find available port starting from 8050
    port = 8056
    while port < 8500:
        try:
            generator.visualize(work_folder='../../results/caida_c0_1-100_relu_act', port=port)
            break
        except Exception:
            port += 1 

    ray.shutdown()
