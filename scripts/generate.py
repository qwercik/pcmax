import random
import subprocess

def randomValue():
	MIN_VALUE = 0
	MAX_VALUE = 10**5
	return random.randint(MIN_VALUE, MAX_VALUE)

def randomList(size):
	return [randomValue() for i in range(size)]

def ascendingList(size):
	return list(range(size))

def descendingList(size):
	return list(range(size, -1, -1))

def constantList(size):
	value = randomValue()
	return [value for i in range(size)]

def aShapeList(size):
	return [i for i in range(int(size / 2))] + [i for i in range(int(size / 2), -1, -1)]

def external(size):
	return [size]

def exec(size):
	output = subprocess.check_output(f'./scripts/instance_generator 4 {size}', shell=True).decode()
	return output.strip().split('\n')

functions = {
	'random': randomList,
	'ascending': ascendingList,
	'descending': descendingList,
	'constant': constantList,
	'a-shape': aShapeList,
	'external': external,
	'exec': exec
}

