import sys
if sys.version_info[0] < 3:
    import Tkinter as tk
else:
	import tkinter as tk

import StimConstructor
import os
import json
import multiprocessing

def load_stim(pi_file):
	# return the list of blocks in the file pi_file
	data = json.load(pi_file)
	block_list = []
	for block_attributes in data:
		block_list.append(StimConstructor.load_block(block_attributes))
	for block in block_list:
		print block.return_commands()
	return block_list

def read_stim(block_list):
	# reads a list of blocks, and sends the protocols sequentially, each for the duration within its "duration" attribute
	for block in block_list:
		pass
	pass

master = tk.Tk()
stim_list = tk.StringVar(master)
stim_list.set("No stimulus")
available_stimuli = [os.path.splitext(file)[0] for file in os.listdir("stimuli") if file.endswith(".pi")]
stim_menu = tk.OptionMenu(master,stim_list, *available_stimuli)
stim_menu.pack(side=tk.LEFT)
StimConstructor.StimConstructor(tk.Toplevel(master),"Paired pulse",["Red","Green","Blue"])
load = tk.Button(master,text="Load stimulus", command=lambda: load_stim(open("stimuli//%s.pi" %stim_list.get())))
load.pack(side=tk.RIGHT)

master.mainloop()
