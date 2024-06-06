import time
import torch

print("Current time: {}".format(time.time()))
time.sleep(3)
print(torch.cuda.is_available())
print("Current time: {}".format(time.time()))
