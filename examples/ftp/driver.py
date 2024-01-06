
import random
import netshare.ray as ray
from netshare import Generator

if __name__ == '__main__':
    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    generator = Generator(config="config_ftp_nodp.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    work_folder = '../../results/ftp'
    generator.train(work_folder=work_folder, preprocess=False, train=True)
    generator.generate(work_folder=work_folder)
    generator.visualize(work_folder=work_folder, port=8054)

    ray.shutdown()
