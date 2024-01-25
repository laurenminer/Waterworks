## Gui for selecting the stimulus to use (stored on the Raspberry Pi)
import sys
if sys.version_info[0] < 3:
    import Tkinter as tk
else:
	import tkinter as tk
import StimConstructor
import os
import json
from StimulusBlock import StimulusBlock

class StimSelector(object):
	def __init__(self,command_window,pi,stim_string):
		# Select a stimulus then pass it to the string variable stim_string
		self.command_window = command_window
		self.window = tk.Toplevel(command_window.window)
		self.pi = pi
		self.stim_string =stim_string
		# blocks are stored as a list of blocks, in "order of appearance"
		self.blocks = []
		self.stimuli = self.pi.retrieve_stim_dict(command_window.protocol)
		# main frame
		initialFrame = tk.Frame(self.window)
		initialFrame.pack(side=tk.TOP)
		# frame for the block info
		data_frame = tk.Frame(initialFrame)
		data_frame.pack(side=tk.TOP)
		# frame for the list of protocols
		scroll_frame = tk.Frame(data_frame)
		scroll_frame.pack(side=tk.LEFT)
		scrollbar = tk.Scrollbar(scroll_frame)
		scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
		self.listbox = tk.Listbox(scroll_frame, yscrollcommand=scrollbar.set)
		for stim in self.stimuli.keys():
			self.listbox.insert(tk.END,stim)
		self.listbox.pack(side=tk.LEFT, fill=tk.BOTH)
		self.listbox.bind("<<ListboxSelect>>", self.onselect)
		scrollbar.config(command=self.listbox.yview)
		# frame to preview stimuli
		self.preview_frame = tk.Frame(data_frame)
		self.preview_frame.pack(side=tk.RIGHT)

		#frame for buttons
		self.button_frame = tk.Frame(initialFrame)
		self.button_frame.pack(side=tk.BOTTOM)
		select_button = tk.Button(self.button_frame, text="Select stimulus", command = lambda: self.select_stim())
		select_button.pack(side=tk.LEFT)
		delete_button = tk.Button(self.button_frame, text="Delete stimulus", command = lambda: self.delete_stim())
		delete_button.pack(side=tk.RIGHT)

	def select_stim(self):
		self.stim_string.set(self.listbox.get(self.listbox.curselection()))
		self.window.destroy()

	def delete_stim(self):
		selected_stimulus = self.listbox.get(self.listbox.curselection())
		self.pi.sftp_client.remove("./stimuli/%s/%s" %(self.command_window.protocol, selected_stimulus))
		# update the menu
		self.stimuli = self.pi.retrieve_stim_dict(self.command_window.protocol)
		self.listbox.delete(0, tk.END)
		for stim in self.stimuli.keys():
			self.listbox.insert(tk.END,stim)

	def onselect(self,event):
		w = event.widget
		stim_name = w.get(int(w.curselection()[0]))
		# get the updated stim dict
		self.stim_dict = self.pi.retrieve_stim_dict(self.command_window.protocol)
		self.stimulus_set = self.stim_dict[stim_name] # returns the stimulus itself as a list of blocks
		self.num_blocks = len(self.stimulus_set)
		self.curr_block = 1

		# for previewing content of current block
		try:
			self.block_container.destroy()
			self.block_frame.destroy()
			self.key_frame.destroy()
		except:
			pass
		self.block_frame = tk.Frame(self.preview_frame)
		self.block_frame.pack(side=tk.TOP)
		self.block_container = tk.Frame(self.block_frame)
		self.block_container.pack()
		self.visualize_block()

		# for selecting which block is shown
		self.key_frame = tk.Frame(self.preview_frame)
		self.key_frame.pack(side=tk.BOTTOM)
		if self.num_blocks > 1:
			self.right_block = tk.Button(self.key_frame, text="->", command = lambda: self.next_block())
			self.right_block.pack(anchor=tk.E)
		self.block_string = tk.StringVar()
		self.update_block_string()
		self.block_string_label = tk.Label(self.key_frame, textvariable = self.block_string)
		self.block_string_label.pack(anchor=tk.CENTER)

	def visualize_block(self):
		self.block_container.destroy()
		self.block_container = tk.Frame(self.block_frame)
		self.block_container.pack()
		block = self.stimulus_set[self.curr_block - 1] # indexing
		# Copied from StimConstructor code
		tk.Label(self.block_container,text="Duration: %f minutes" % block.duration).pack(side=tk.TOP)
		for color in block.colors:
			# window for each color
			color_frame = tk.Frame(self.block_container)
			color_frame.pack(side=tk.LEFT)
			tk.Label(color_frame,text=color).pack(side=tk.TOP)
			for field in block.param_fields:
				tk.Label(color_frame,text="%s: %s" % (field, block.color_params[color][field])).pack(side=tk.TOP)

	def next_block(self):
		if self.curr_block == 1:
			self.left_block = tk.Button(self.key_frame, text="<-", command = lambda: self.previous_block())
			self.left_block.pack(anchor=tk.W)
		if self.curr_block == (self.num_blocks - 1):
			self.right_block.destroy()
		self.curr_block += 1
		self.update_block_string()
		self.visualize_block()

	def previous_block(self):
		if self.curr_block == 2:
			self.left_block.destroy()
		if self.curr_block == (self.num_blocks):
			self.right_block = tk.Button(self.key_frame, text="->", command = lambda: self.next_block())
			self.right_block.pack(anchor=tk.E)
		self.curr_block -= 1
		self.update_block_string()
		self.visualize_block()

	def update_block_string(self):
		self.block_string.set("%s/%s" %(self.curr_block,self.num_blocks))

#StimConstructor.load_block(attributes) # reads a block