import random
import netshare.ray as ray
from netshare import Generator
import os 

if __name__ == '__main__':
    # cd to folder
    os.chdir("/home/xinyu/NetShareTorch/examples/youtube")    

    # set to gpu 0
    os.environ["CUDA_VISIBLE_DEVICES"] = "0"

    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    generator = Generator(config="config_youtube_nodp.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    # generator.train(work_folder='../../results/youtube_smallbatch', preprocess=False)
    # exit()
    generator.generate(work_folder='../../results/youtube_smallbatch')
    generator.visualize(work_folder='../../results/youtube_smallbatch', port=8051)

    ray.shutdown()
