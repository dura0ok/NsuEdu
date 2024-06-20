import subprocess


num_iterations = 100


a = 1  
b = 1000  

c_program_path = "./a.out"

def run_c_program(tv_usec_value):
    try:
        result = subprocess.run(
            [c_program_path],
            env={"TV_USEC": str(tv_usec_value)},  
            stderr=subprocess.PIPE,
            stdout=subprocess.PIPE,
            universal_newlines=True,
            check=True,
        )
        print(f"Successfully ran with tv_usec = {tv_usec_value}")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error running with tv_usec = {tv_usec_value}: {e.stderr}")
        return False


for i in range(num_iterations):
    tv_usec_value = a + (i % (b - a + 1))
    if not run_c_program(tv_usec_value):
        continue
