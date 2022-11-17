
task.wait(3)

local uis = game:GetService("UserInputService")

local num = 0
local decimal = 0
local enabled = 0

local button_states = {1, 1, 1, 1}
local previous_states = {1, 1, 1, 1}
local last_times = {0, 0, 0, 0}
local held_pins = {0, 0, 0, 0}

local debounce_cycles = 0
local debounce_time = 2
local timer_counter = 0

local function update_inputs()
	for pin = 1, 4 do
		local buttonObject = script.Parent:FindFirstChild(tostring(pin))
		
		local button_state = buttonObject:GetAttribute("clicked")
		local previous_state = previous_states[pin]
		
		if previous_state ~= button_state then
			last_times[pin] = debounce_cycles
		end
		
		if (debounce_cycles - last_times[pin]) >= debounce_time then
			if button_state ~= button_states[pin] then
				button_states[pin] = button_state
				if button_state == 1 then
					held_pins[pin] = 0
				end
			end
		end
		
		previous_states[pin] = button_state
	end
end

local function update_display(tens, ones, tenths, hundredths)
	script.Parent.display.Text = tens .. ones .. "." .. tenths .. hundredths
end

do
	for _, obj in ipairs(script.Parent:GetChildren()) do
		if tonumber(obj.Name) then
			obj.MouseButton1Down:Connect(function()
				obj:SetAttribute("clicked", 0)
			end)
			obj.MouseButton1Up:Connect(function()
				obj:SetAttribute("clicked", 1)
			end)
		end
	end
end

task.spawn(function()
	while true do
		task.wait(0.05)
		
		if enabled == 1 then
			timer_counter = timer_counter + 1
			decimal = decimal + 0.05
		end
		
		if timer_counter >= 20 then
			num = num + 1
			decimal = 0
			timer_counter = 0
		end
	end
end)

while true do
	task.wait()
	
	if (num > 59) then
		num = 0
	elseif (num < 0) then
		num = 59
	end
	if decimal >= 1 then
		decimal = 0
	end
	
	local tens = math.floor(num / 10)
	local ones = math.floor(num % 10)
	local decimal_whole = decimal * 100
	local tenths = math.floor(decimal_whole / 10)
	local hundredths = math.floor(decimal_whole % 10)
	debounce_cycles = debounce_cycles + 1
	
	update_display(tens, ones, tenths, hundredths)
	update_inputs()
	
	if (button_states[1] == 0) and (held_pins[1] == 0) then
		num = num + 1
		held_pins[1] = 1
	end
	if (button_states[2] == 0) and (held_pins[2] == 0) then
		num = num - 1
		held_pins[2] = 1
	end
	if (button_states[3] == 0) and (held_pins[3] == 0) then
		num = 0
		decimal = 0
		held_pins[3] = 1
	end
	if (button_states[4] == 0) and (held_pins[4] == 0) then
		enabled = (enabled == 0) and 1 or 0
		held_pins[4] = 1
	end
end
