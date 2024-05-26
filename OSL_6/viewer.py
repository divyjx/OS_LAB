import cv2
import matplotlib.pyplot as plt
img = cv2.imread("\path to the image")
# Remember, opencv by default reads images in BGR rather than RGB
# So we fix that by the following
img = cv2.cvtColor(img,cv2.COLOR_BGR2RGB)
# Now, for small images like yours or any similar ones we use for example purpose to understand image processing operations or computer graphics
# Using opencv's cv2.imshow()
# Or google.colab.patches.cv2_imshow() [in case we are on Google Colab]
# Would not be of much use as the output would be very small to visualize
# Instead using matplotlib.pyplot.imshow() would give a decent visualization
plt.imshow(img)
