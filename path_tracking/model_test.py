from data_processing import *
from torchvision import transforms
from torch.utils.data import DataLoader, random_split
import torch.optim as optim
from model import *
import cv2

from PIL import Image

import numpy as np
import os

from time import perf_counter
# custom_weights = 'path_tracking/custom_models/model_low_loss_epoch_35.pth'
custom_weights = "path_tracking\custom_models\/test4_vel_ratio\model_low_loss_epoch_30.pth"

custom_model = CNNRegressor()
custom_model.load_state_dict(torch.load(custom_weights))
custom_model.eval()

mn_weights = "path_tracking\mobilenetv3\/test2_vel_ratio\model_low_loss_epoch_33.pth"
mn_model = getMobileNet(isPretrained=False, grayscale=True, weights_path=mn_weights)
mn_model.eval()

min_log_vel = -1.6699729967118622
max_log_vel = 1.0093420117490313

custom_data_transform = transforms.Compose([
    transforms.Grayscale(num_output_channels=1),  # Convert to grayscale
    transforms.ToTensor(),  # Convert image to tensor
    transforms.Normalize(mean=[0.5], std=[0.5])  # Normalize 
])

mn_data_transform = transforms.Compose([
    transforms.Resize(224), #mobilenet expects this size input
    transforms.Grayscale(num_output_channels=1),  # Convert to grayscale
    transforms.ToTensor(),  # Convert image to tensor
    transforms.Normalize(mean=[0.5], std=[0.5])  # Normalize 
])
path = 'data\curved_flawed\image74.jpg'
image = Image.open(path)
# image = Image.open('data\straight\image19.jpg')

# Apply the transformation
image_1 = custom_data_transform(image).unsqueeze(0)  # Add batch dimension
image_2 = mn_data_transform(image).unsqueeze(0)  # Add batch dimension

with torch.no_grad():
    time_1 = perf_counter()
    output_1 = custom_model(image_1)
    time_2 = perf_counter()
    output_2 = mn_model(image_2)
    time_3 = perf_counter()
    # print(output)  # Output of the model
#
print(path)
print(f"Custom Model inference time: {time_2 - time_1}")
print(f"MobileNet inference time: {time_3 - time_2}")

outputs = [output_1, output_2]


for i, o in enumerate(outputs):
    log_ratio = o * (max_log_vel - min_log_vel) + min_log_vel
    ratio = np.exp(log_ratio)
    print(i+1, ":", ratio)