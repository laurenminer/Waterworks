from tkinter import *
from PIL import ImageTk, Image
import os

root = Tk()
image_path = "cameraman.jpg"
img = ImageTk.PhotoImage(Image.open(image_path))
panel = Label(root, image = img)
panel.image = img
panel.pack(side = "bottom", fill = "both", expand = "yes")
root.mainloop()