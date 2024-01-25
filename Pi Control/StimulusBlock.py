# Stimulus block class

class StimulusBlock(object):
	# A class containing a temporal block of light stimulation
	def __init__(self, protocol, colors):
		# Contains a string which tells you what "type" of stimulation it is
		# ("block", "flashing lights", or "paired pulse" in protocol) and a dictionary for the
		# params, corresponding to the details of stimulation during this block
		self.protocol = protocol
		self.param_fields = self.get_param_fields(protocol)
		self.colors = colors
		self.color_params = {}
		self.buttons = []
		self.duration = 0.0
		self.attributes = {"protocol":self.protocol,"param_fields":self.param_fields,"colors":self.colors,"color_params":self.color_params,
			"buttons":self.buttons,"duration":self.duration}
		# initialize all fields to 0
		for color in self.colors:
			self.color_params[color] = {}
			for field in self.param_fields:
				self.color_params[color][field] = "0"

	def set_duration(self,dur):
		self.duration = dur
		self.attributes["duration"]=self.duration

	def update_params(self, params_to_update,color):
		# updates the parameters
		for (name,param) in params_to_update.iteritems():
			self.color_params[color][name] = param

	def return_commands(self):
		# make a list of the params in order that they should be sent to the Arduino
		command_params = [self.color_params[color][field] for color in self.colors for field in self.param_fields]
		# join each parameter with a comma and concatenate them
		command = ",".join(command_params)
		return command

	def get_param_fields(self,protocol):
		# Return the ordered list of fields used in this block
		if protocol == "Paired pulse":
			return ["Wait (first) (min)", "First pulse (ms)", "Wait (second) (sec)", "Second pulse (ms)"]
		if protocol == "Flashing Lights":
			return ["Frequency (Hz)","Pulse duration (ms)"]
		if protocol == "Blocks":
			return ["Pulse frequency (Hz)", "Pulse duration (ms)", "Block start time (ms)", "Interblock interval (ms)"]
