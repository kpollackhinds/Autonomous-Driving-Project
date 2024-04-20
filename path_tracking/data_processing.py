from PIL import Image
import numpy as np
import glob
import re
from torch.utils.data import Dataset
import os
import torch


class PathFollowingDataset(Dataset):
    def __init__(self, image_folder, velocities, transform=None):
        """
        Args:
            image_folder (string): Directory with all the images.
            velocities (numpy array): Array of normalized velocities.
            transform (callable, optional): Optional transform to be applied on a sample.
        """
        self.image_folder = image_folder
        self.velocities = velocities
        self.transform = transform
        # Sort the files numerically based on the digits in the filenames
        self.image_filenames = [f for f in sorted(os.listdir(image_folder), key=self.numerical_sort) if f.endswith('.jpg')]

    def __len__(self):
        return len(self.image_filenames)

    def __getitem__(self, idx):
        # Load image
        img_path = os.path.join(self.image_folder, self.image_filenames[idx])
        image = Image.open(img_path)  # Convert to RGB if needed, use 'L' for grayscale

        # Apply transformation
        if self.transform:
            image = self.transform(image)

        # Get velocity
        velocity = self.velocities[idx]

        return {'image': torch.tensor(image, dtype=torch.float), 'velocity': torch.tensor(velocity, dtype=torch.float)}
    

    @staticmethod
    def numerical_sort(filename):
        """
        A helper function to extract numbers from a filename and use them for sorting.
        """
        numbers = re.findall(r'\d+', filename)
        return int(numbers[0]) if numbers else 0



def load_and_normalize_image(image_path):
    # Load the image
    with Image.open(image_path) as img:
        # Convert the image to a numpy array
        image_array = np.array(img)

    # Normalize the image array to have pixel values between 0 and 1
    normalized_image_array = image_array / 255.0

    return normalized_image_array

def combine_and_normalize_velocity_files(folder_path):
    # List all .npy files in the folder
    file_paths = glob.glob(f'{folder_path}/velocity_run[0-9]*.npy')
    # Sort files by the numeric value in their names
    file_paths.sort(key=lambda x: int(re.findall(r'(\d+)', x)[0]))

    # Load the numpy arrays
    velocities = [np.load(f) for f in file_paths]

    # Concatenate the numpy arrays
    combined_velocities = np.concatenate(velocities, axis=0)

    # Find the maximum velocity value for normalization
    max_velocity = np.max(combined_velocities)

    # Normalize the velocities
    normalized_velocities = combined_velocities / max_velocity
    return normalized_velocities, max_velocity


if __name__ == "__main__":
    path = 'data/straight'
    n = combine_and_normalize_velocity_files(path)
    # print(np.info(n))