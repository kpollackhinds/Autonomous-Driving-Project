import torch
from torch import nn 

# Reference for determining FC layer size after 2D Conv 
# https://datascience.stackexchange.com/questions/40906/determining-size-of-fc-layer-after-conv-layer-in-pytorch

class CNNRegressor(nn.Module):

    def __init__(self):
        super(CNNRegressor, self).__init__()
        
        # 1 input image channel because grayscale
        self.conv1 = nn.Conv2d(1, 8, 5)
        self.batch_norm = nn.BatchNorm2d(8)
        self.relu = nn.ReLU()
        self.pool = nn.MaxPool2d(2,2)

        self.conv2 =  nn.Conv2d(8, 16, 5)

        self.conv3 = nn.Conv2d(16, 32, 5)
        
        self.dropout = nn.Dropout2d(0.2)

        #will determine empirically
        self.fc1 = nn.Linear(16, )  
        self.fc2 = nn.Linear()
        self.fc3 = nn.Linear(_, 2)
    
    def forward(self, x):
        x = self.conv1(x)
        x = self.batch_norm(x)
        x = self.relu(x)
        x = self.pool(x)

        x = self.conv2(x)
        x=self.batch_norm(x)
        x = self.relu(x)
        x = self.pool(x)

        x = self.conv3(x)
        x = self.batch_norm(x)
        x = self.relu(x)
        x = nn.Flatten(x)

        x = self.fc1(x)
        x = self.fc2(x)
        x = self.fc3(x)

        return x

