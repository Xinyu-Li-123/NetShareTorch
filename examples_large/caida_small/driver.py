import random
import netshare.ray as ray
from netshare import Generator
import os 

if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.abspath(__file__)))

    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    generator = Generator(config="config_caida_small_nodp.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    generator.train(work_folder='../../results/caida_small', preprocess=True, train=True)
    generator.generate(work_folder='../../results/caida_small')
    generator.visualize(work_folder='../../results/caida_small', port=8058)

    ray.shutdown()
