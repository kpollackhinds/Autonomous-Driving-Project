import torch
from torch import nn 

import torchvision.models as models
from torchvision.models import mobilenet_v3_small, MobileNet_V3_Small_Weights


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
        # input_features = 32 * 5 * 5  # This needs to be calculated based on your input size and conv/pool operations
        self.fc1 = nn.Linear(123808, 120)
        self.fc2 = nn.Linear(120, 84)
        self.fc3 = nn.Linear(84, 1)  # Output 2 for the two velocity components
    
    def forward(self, x):
        x = self.conv1(x)
        x = self.bn1(x)
        x = self.relu(x)
        # print("p1, " ,x.shape)

        x = self.pool(x)
        # print("c2, " ,x.shape)
        x = self.conv2(x)
        x = self.bn2(x)
        x = self.relu(x)
        # print("p2, " ,x.shape)

        x = self.pool(x)
        # print("c3, " ,x.shape)

        x = self.conv3(x)
        x = self.bn3(x)
        x = self.relu(x)
        # print("fc1, " ,x.shape)

        x = self.dropout(x)
        x = self.flatten(x)
        x = self.fc1(x)
        # print("fc2, " ,x.shape)

        x = self.fc2(x)
        # print("fc3, " ,x.shape)

        x = self.fc3(x)

        return x

def getMobileNet(isPretrained=False, grayscale = True, weights_path = None):
    if isPretrained:
        # Load the default pre-trained weights
        weights = MobileNet_V3_Small_Weights.DEFAULT
        model = mobilenet_v3_small(weights=weights)
    else:
        # Initialize without pre-trained weights
        model = mobilenet_v3_small(weights=None)
    # changing the first layer to fit gray scaled inputs:
    if grayscale:
        first_conv_layer = model.features[0][0]
        # really just making the input layer size 1 instead of 3
        new_first_layer = torch.nn.Conv2d(1, first_conv_layer.out_channels, 
                                           kernel_size=first_conv_layer.kernel_size, 
                                           stride=first_conv_layer.stride, 
                                           padding=first_conv_layer.padding, 
                                           bias=False)
        
        model.features[0][0] = new_first_layer

    # Remove the last classification layer and replace it with a new layer for regression
    model.classifier[3] = nn.Linear(model.classifier[3].in_features, 1)  # 2 outputs for velocities

    # # Optionally, you can start training with frozen pre-trained layers first
    # for param in model.features.parameters():
    #     param.requires_grad = False  # Freeze feature layers

    # # When ready, you can unfreeze these layers for fine-tuning
    # for param in model.features.parameters():
    #     param.requires_grad = True  # Unfreeze feature layers for fine-tuning

    # load model after training
    if weights_path:
        model.load_state_dict(torch.load(weights_path))
    return model


