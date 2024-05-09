from PIL import Image
import numpy as np
import glob
import re
from torch.utils.data import Dataset
import os
import torch


class PathFollowingDataset(Dataset):
    def __init__(self, image_filenames, velocities, transform=None):
        """
        Args:
            velocities (numpy array): Array of normalized velocities.
            transform (callable, optional): Optional transform to be applied on a sample.
        """
        self.image_filenames = image_filenames
        self.velocities = velocities
        self.transform = transform
        # # Sort the files numerically based on the digits in the filenames
        # self.image_filenames = [f for f in sorted(os.listdir(image_folder), key=self.numerical_sort) if f.endswith('.jpg')]

    def __len__(self):
        return len(self.image_filenames)

    def __getitem__(self, idx):
        # Load image
        # img_path = os.path.join(self.image_folder, self.image_filenames[idx])
        image = Image.open(self.image_filenames[idx])  # Convert to RGB if needed, use 'L' for grayscale

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

def combine_and_normalize_velocity_files(folder_path, normalize = True, log_transform = True):
    # List all .npy files in the folder
    file_paths = glob.glob(f'{folder_path}/velocity_run[0-9]*.npy')
    # Sort files by the numeric value in their names
    file_paths.sort(key=lambda x: int(re.findall(r'(\d+)', x)[0]))
    image_filenames = sorted(glob.glob(f'{folder_path}/image*.jpg'), key= lambda x : int(re.findall(r'(\d+)', x)[0]))

    # Load the numpy arrays
    velocities = [np.load(f) for f in file_paths]
    combined_velocities = np.concatenate(velocities, axis=0)
    
    # getting rid of erroneously included negative velocities
    valid_indices = np.all(combined_velocities > 0, axis=1)
    combined_velocities = combined_velocities[valid_indices]
    filtered_image_filenames = [image_filenames[i] for i in np.where(valid_indices)[0]]

    # Find the maximum velocity value for normalization
    max_velocity = np.max(combined_velocities)
    min_velocity = np.min(combined_velocities)

    # Count negative velocities
    negative_count = np.sum(combined_velocities < 0)

    print(f"Minimum velocity: {min_velocity}")
    print(f"Maximum velocity: {max_velocity}")
    print(f"Number of negative velocities: {negative_count}")     

    print(combined_velocities[135:140])

    # Difference method
    # relative_velocities = combined_velocities[:, 0] - combined_velocities[:, 1]

    # Ratio Method
    relative_velocities = np.divide(combined_velocities[:,0], combined_velocities[:,1])
    print(relative_velocities[135:140])
    
    print(f"Minimum rel velocity: {np.min(relative_velocities)}")
    print(f"Maximum rel velocity: {np.max(relative_velocities)}")

    if normalize:
        if log_transform:
            relative_velocities = np.log(relative_velocities)
            print(f"Minimum log velocity: {np.min(relative_velocities)}")
            print(f"Maximum log velocity: {np.max(relative_velocities)}")
        
        # normalized_velocities = combined_velocities / max_velocity
        # normalized_velocities = (combined_velocities - min_velocity) / (max_velocity - min_velocity)
        normalized_velocities = (relative_velocities - np.min(relative_velocities)) / (np.max(relative_velocities) - np.min(relative_velocities))
        print(f"Minimum normalized velocity: {np.min(normalized_velocities)}")
        print(f"Maximum normalized velocity: {np.max(normalized_velocities)}")

        normalized_midpoint = (((np.min(relative_velocities) + np.max(relative_velocities))*0.5) - np.min(relative_velocities))/(np.max(relative_velocities) - np.min(relative_velocities))
        print(f"Normalized midpoint: {normalized_midpoint}")

        return normalized_velocities, filtered_image_filenames, normalized_midpoint
        
    else:
        return combined_velocities, filtered_image_filenames


if __name__ == "__main__":
    path = 'data'
    n, v, m = combine_and_normalize_velocity_files(path, log_transform=True)
    # print(np.info(n))