from shutil import copy, rmtree
from subprocess import Popen
from pathlib import Path
from os import system


def safety_rmtree(path):
	try:
		rmtree(path)
	except:
		pass


def get_priorities(option):
	if option == 1:
		return "5", "5", "5"
	elif option == 2:
		return "0", "5", "5"
	elif option == 3:
		return "0", "5", "1"
	else:
		raise Exception(f"Unknown option: {option}")


def main():
	for core in range(1, 3 + 1):
		for option in range(1, 3 + 1):              
			path = Path(f"./{core}cores_{option}option")
			safety_rmtree(path)
			path.mkdir()
			copy(Path("./lw4.exe"), path)
			copy(Path("./../EDTV.bmp"), path)
			Popen(["lw4", "EDTV.bmp", "out.bmp", "3", str(core), *get_priorities(option)], cwd=path).wait()
			for thread in range(0, 2 + 1):
				with open(f"{path}/thread_{thread}.log", "r") as log:
					timings = [int(line) for line in log.read().splitlines()]
					timings = [str(timing) for timing in sorted(list(set(timings)))]
					with open(f"{path}/unique_thread{thread}.log", "w") as unique_log:
						unique_log.write("\n".join(timings))
					
					
				


if __name__ == "__main__":
	main()
