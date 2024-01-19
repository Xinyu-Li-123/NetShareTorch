import random
import netshare.ray as ray
from netshare import Generator
import torch 

if __name__ == '__main__':
    
    # # disable GPU, force to use cpu-only to train model
    # torch.cuda.is_available = lambda : False

    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    generator = Generator(config="config_dc_c0_nodp.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    generator.train(work_folder='../../results/dc_c0', preprocess=True, train=True)
    generator.generate(work_folder='../../results/dc_c0')
    generator.visualize(work_folder='../../results/dc_c0')

    ray.shutdown()
