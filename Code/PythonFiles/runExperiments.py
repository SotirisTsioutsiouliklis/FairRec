import subprocess

datasets = ['blogs', 'dblp_course', 'tmdb', 'twitter', 'physics', 'linkedin', 'dblp_aminer']

for dataset in datasets:
    # Copy executables.
    subprocess.call(["cp ../../Executables/CppFiles/experimentOne.out"], cwd="%s" %dataset, shell=True)
    subprocess.call(["cp ../../Executables/CppFiles/experimentTwo.out"], cwd="%s" %dataset, shell=True)
    subprocess.call(["cp ../../Executables/CppFiles/experimentThree.out"], cwd="%s" %dataset, shell=True)
    # subprocess.call(["cp ../../Executables/PythonFiles/"], cwd="%s" %dataset, shell=True)
    # Run executables.
    subprocess.call(["/.experimentOne.out"], cwd="%s" %dataset, shell=True)
    subprocess.call(["/.experimentTwo.out"], cwd="%s" %dataset, shell=True)
    subprocess.call(["/.experimentOThree.out"], cwd="%s" %dataset, shell=True)
