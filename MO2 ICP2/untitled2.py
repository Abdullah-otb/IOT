

from google.colab import drive
drive.mount('/content/drive')

class_name = {'0':'Dog','1':'Rooster','10':'Rain','11':'Sea Waves','12':'Crackling Fire','20':'Crying Baby','21':'Sneezing','38':'Clock Tick','40':'Helicopter','41':'Chainsaw'}

import pandas as pd

my_df = pd.read_csv('/content/drive/My Drive/esc50.csv')

my_df = my_df[my_df['esc10']==True]

#import the pyplot and wavfile modules 

import matplotlib.pyplot as plt
import os
from scipy.io import wavfile

def WAVtoPNG(filename):
  samplingFrequency, signalData = wavfile.read('/content/drive/My Drive/Colab Notebooks/ESC-50-master/audio/'+filename)
  filename = filename.split('.')
  fig = plt.figure(frameon=False)
  ax = plt.Axes(fig, [0., 0., 1., 1.])
  ax.set_axis_off()
  fig.add_axes(ax)
  _ = plt.specgram(signalData,Fs=samplingFrequency,cmap='jet')


  if filename[0][0] != '5':
    if os.path.isdir("/content/drive/My Drive/WAV_Image/Train/"+filename[0].split('-')[-1]) == False:
      os.makedirs("/content/drive/My Drive/WAV_Image/Train/"+filename[0].split('-')[-1])
    fig.savefig('/content/drive/My Drive/WAV_Image/Train/'+filename[0].split('-')[-1]+'/'+filename[0]+".png")
  else:
    if os.path.isdir("/content/drive/My Drive/WAV_Image/Valid/"+filename[0].split('-')[-1]) == False:
      os.makedirs("/content/drive/My Drive/WAV_Image/Valid/"+filename[0].split('-')[-1])
    fig.savefig('/content/drive/My Drive/WAV_Image/Valid/'+filename[0].split('-')[-1]+'/'+filename[0]+".png")


# Read the wav file (mono) to
for filename in my_df['filename']:
  WAVtoPNG(filename)

import os
from PIL import Image

rootdir = '/content/drive/My Drive/WAV_Image'
count = 0
for subdir, dirs, files in os.walk(rootdir):
    for file in files:
      #print(os.path.join(subdir, file))
      count += 1
print(count)

from torchvision import transforms, datasets, models
import torch
import os
from torch import nn
import torch.nn.functional as F

data_transforms = {
    'Train': transforms.Compose(
    [transforms.ToTensor(),
     transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])
    ,
    'Valid': transforms.Compose(
    [transforms.ToTensor(),
     transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])
}
# TODO: Load the datasets with ImageFolder
image_datasets = {x: datasets.ImageFolder(os.path.join('/content/drive/My Drive/WAV_Image', x), data_transforms[x]) for x in ['Train']}
# TODO: Using the image datasets and the trainforms, define the dataloaders
batch_size = 4
trainloader = {x: torch.utils.data.DataLoader(image_datasets[x], batch_size=batch_size,
                                             shuffle=True, num_workers=4)
              for x in ['Train']}

# TODO: Load the datasets with ImageFolder
test_datasets = {x: datasets.ImageFolder(os.path.join('/content/drive/My Drive/WAV_Image', x), data_transforms[x]) for x in ['Valid']}
# TODO: Using the image datasets and the trainforms, define the dataloaders
batch_size = 4
testloader = {x: torch.utils.data.DataLoader(test_datasets['Valid'], batch_size=batch_size,
                                             shuffle=True, num_workers=4)
for x in ['Valid']}

class_names = image_datasets['Train'].classes
dataset_sizes = {x: len(image_datasets[x]) for x in ['Train']}
class_names = image_datasets['Train'].classes

#Set GPU
device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

#Send Model to GPU
vgg19 = (models.vgg19(pretrained=False)).to(device)

import torch.optim as optim

criterion = nn.CrossEntropyLoss()
optimizer = optim.SGD(vgg19.parameters(), lr=0.001, momentum=0.9)

for epoch in range(73):  # loop over the dataset multiple times
      i=0
      running_loss = 0.0
      for images, labels in trainloader['Train']:
        optimizer.zero_grad()
        images, labels = images.to(device), labels.to(device)
        #images = images.view(images.shape[0], -1)
        log_ps = vgg19(images)
        loss = criterion(log_ps, labels)
        loss.backward()
        optimizer.step()
        
        running_loss += loss.item()

        i=i+1
        # print statistics
        if i % 10 == 9:    # print every 2000 mini-batches
            print('[%d, %5d] loss: %.3f' %
                  (epoch + 1, i + 1, running_loss / 10))
            running_loss = 0.0

print('Finished Training')

class_correct = list(0. for i in range(10))
class_total = list(0. for i in range(10))
with torch.no_grad():
    for data in testloader['Valid']:
        images, labels = data
        images, labels = images.to(device), labels.to(device)
        #images = images.view(images.shape[0], -1)
        outputs = vgg19(images)
        _, predicted = torch.max(outputs, 1)
        c = (predicted == labels).squeeze()
        for i in range(4):
            label = labels[i]
            class_correct[label] += c[i].item()
            class_total[label] += 1


for i in range(10):
    print('Accuracy of %5s : %2d %%' % (
        class_name[class_names[i]], 100 * class_correct[i] / class_total[i]))

model_save_name = 'ESC10classify.pth'
path = "/content/drive/My Drive/Documents/{model_save_name}" 
torch.save(vgg19.state_dict(), path)



def crop(image_path):

    image_obj = Image.open(image_path)
    cropped_image = image_obj.crop((width1, 0, width2, image_obj.height))
    cropped_image.save(image_path)

import os
from PIL import Image

rootdir = '/content/drive/My Drive/WAV_Image'

for subdir, dirs, files in os.walk(rootdir):
    for file in files:
      im = Image.open(os.path.join(subdir, file))
      rgb_im = im.convert('RGB')
      flag1 = False
      flag2 = False
      width1, width2 = 0, im.width
      #print('inhere')
      for w in range(im.width):
        r, g, b = rgb_im.getpixel((w, 1))
        if(r == g == b and flag1 == False):
          flag1 = True
        elif(flag2 == False):
          width1 = w
          flag2 = True
        elif(r == g == b and flag1 == True):
          width2 = w
          break
      
      crop(os.path.join(subdir, file))

