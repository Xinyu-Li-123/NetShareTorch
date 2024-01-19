import random
import netshare.ray as ray
from netshare import Generator
import os 

if __name__ == '__main__':
    # cd to folder
    os.chdir("/home/xinyu/NetShareTorch/examples/youtube_small")    

    # set to gpu 1
    os.environ["CUDA_VISIBLE_DEVICES"] = "0"

    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    generator = Generator(config="config_youtube_small_nodp.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    # generator.train(work_folder='../../results/youtube_small', preprocess=True, train=True)
    # exit()
    generator.generate(work_folder='../../results/youtube_small')
    generator.visualize(work_folder='../../results/youtube_small')

    ray.shutdown()
