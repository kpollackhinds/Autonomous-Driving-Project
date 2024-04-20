import torch
from torch import nn 

# Reference for determining FC layer size after 2D Conv 
# https://datascience.stackexchange.com/questions/40906/determining-size-of-fc-layer-after-conv-layer-in-pytorch

class CNNRegressor(nn.Module):

    def __init__(self):
        super(CNNRegressor, self).__init__()
        
        # 1 input image channel because grayscale
        self.conv1 = nn.Conv2d(1, 8, 5)
        self.bn1 = nn.BatchNorm2d(8)
        self.relu = nn.ReLU()
        self.pool = nn.MaxPool2d(2,2)

        self.conv2 =  nn.Conv2d(8, 16, 5)
        self.bn2 = nn.BatchNorm2d(16)

        self.conv3 = nn.Conv2d(16, 32, 5)
        self.bn3 = nn.BatchNorm2d(32)

        self.dropout = nn.Dropout2d(0.2)
        self.flatten = nn.Flatten()

        # #will determine empirically
        # self.fc1 = nn.Linear(16, )  
        # self.fc2 = nn.Linear()
        # self.fc3 = nn.Linear(_, 2)

        # Fully connected layers
        # Assuming output from conv3 is 32 channels; dimensions need calculation
        # Example: If output from conv3 (after pooling) is (32, H, W), determine H and W to set input_features
        input_features = 32 * 5 * 5  # This needs to be calculated based on your input size and conv/pool operations
        self.fc1 = nn.Linear(input_features, 120)
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, 2)  # Output 2 for the two velocity components
    
    def forward(self, x):
        x = self.conv1(x)
        x = self.bn1(x)
        x = self.relu(x)
        x = self.pool(x)

        x = self.conv2(x)
        x = self.bn2(x)
        x = self.relu(x)
        x = self.pool(x)

        x = self.conv3(x)
        x = self.bn3(x)
        x = self.relu(x)

        x = self.dropout(x)
        x = self.flatten(x)
        print(x.shape)
        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)

        return x

