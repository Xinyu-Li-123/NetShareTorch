import random
import netshare.ray as ray
from netshare import Generator

if __name__ == '__main__':
    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    generator = Generator(config="config_ugr16_nodp.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    # 790k flows, 500 epoch (cmp. CAIDA 90k flows, 1000 epoch)
    generator.train(work_folder=f'../../results/ugr16', preprocess=False, train=True)
    generator.generate(work_folder=f'../../results/ugr16')
    # generator.visualize(work_folder=f'../../results/ugr16')

    ray.shutdown()
