"""
Iteration - Epoch conversion:
    Since there is no good measure to convert iteration (used by the NetShare paper) and epoch (used by the newer version of NetShare implemented with PyTorch),
        we use the following formula to convert the iteration to epoch:

        epoch = iteration * batch_size / total_samples / n_chunk

    The n_chunk term is used to account for the fact that we are using chunking to train the model.
    Although the number of flows in each chunk is not exactly 1/10 of the total number of flows, there is no other good measure since the number of flows in each chunk is different,
        and we can't just use the number of flows in chunk 0 to determine the epoch number.
"""
import random
import netshare.ray as ray
from netshare import Generator

if __name__ == '__main__':
    # Change to False if you would not like to use Ray
    ray.config.enabled = False
    ray.init(address="auto")

    # configuration file
    generator = Generator(config="config_caida_nodp.json")

    # `work_folder` should not exist o/w an overwrite error will be thrown.
    # Please set the `worker_folder` as *absolute path*
    # if you are using Ray with multi-machine setup
    # since Ray has bugs when dealing with relative paths.
    work_folder = '/scratch/xl3665/results/caida'
    generator.train(work_folder=work_folder, preprocess=True, train=True)
    generator.generate(work_folder=work_folder)
    # generator.visualize(work_folder=work_folder, port=8054)

    ray.shutdown()
