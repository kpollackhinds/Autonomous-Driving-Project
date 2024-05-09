from data_processing import *
from torchvision import transforms
from torch.utils.data import DataLoader, random_split
import torch.optim as optim
from model import *
import cv2

from PIL import Image

import numpy as np
import os

# custom_weights = 'path_tracking/custom_models/model_low_loss_epoch_35.pth'
custom_weights = 'path_tracking/model_low_loss_epoch_29.pth'

custom_model = CNNRegressor()
custom_model.load_state_dict(torch.load(custom_weights))
custom_model.eval()

mn_weights = "path_tracking\mobilenetv3\model_low_loss_epoch_3.pth"
mn_model = getMobileNet(isPretrained=False, grayscale=True, weights_path=mn_weights)
mn_model.eval()

custom_data_transform = transforms.Compose([
    transforms.Grayscale(num_output_channels=1),  # Convert to grayscale
    transforms.ToTensor(),  # Convert image to tensor
    transforms.Normalize(mean=[0.5], std=[0.5])  # Normalize 
])

# mn_data_transform = transforms.Compose([
#     transforms.Resize(224), #mobilenet expects this size input
#     transforms.Grayscale(num_output_channels=1),  # Convert to grayscale
#     transforms.ToTensor(),  # Convert image to tensor
#     transforms.Normalize(mean=[0.5], std=[0.5])  # Normalize 
# ])

image = Image.open('data\image275.jpg')
# image = Image.open('data\straight\image19.jpg')

# Apply the transformation
image = custom_data_transform(image).unsqueeze(0)  # Add batch dimension
# image = mn_data_transform(image).unsqueeze(0)  # Add batch dimension

with torch.no_grad():
    output = custom_model(image)
    # print(output)  # Output of the model
# 
min_log_vel = -1.6699729967118622
max_log_vel = 1.0093420117490313

log_ratio = output * (max_log_vel - min_log_vel) + min_log_vel
ratio = np.exp(log_ratio)
print(ratio)