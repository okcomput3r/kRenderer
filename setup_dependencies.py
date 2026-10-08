import os
import sys
import platform
import subprocess
from pathlib import Path


def check_os():
    print ("[] Checking O.S")

    system = platform.system()
 
    if 'Linux' in system:
        print("[] Linux O.S detected")

    elif 'win32' in system:
        print('[] Windows O.S detected')

    elif 'darwin' in system:
        print('[] Apple O.S detected')

    return system

def install_vulkan_dependencies():
    print ('[] Installing Vulkan dependencies')

    system = check_os()
    if 'Linux' in system:
        install_vulkan_dependencies_linux()
    elif 'win32' in system:
        install_vulkan_dependencies_win32()
    elif 'darwin' in system:
        install_vulkan_dependencies_apple()

    print ('[] Vulkan dependencies succesfully installed')

    return

def install_vulkan_dependencies_win32():
    print('[] Installing windows dependencies')

    # clone the vulkan tutorial with the dependencies srcipt
    repo_url = 'https://github.com/KhronosGroup/Vulkan-Tutorial'
    install_dir = Path(os.path.abspath(os.path.dirname(sys.argv[0]))) / 'extern' / 'Vulkan-Tutorial'
    if not (install_dir / ".git").exists():
        print('[] Clonning Vulkan-Tutorial repo to : ', str(install_dir))
        subprocess.run(['git', 'clone', repo_url, str(install_dir)], check = True)

    print ('[] Vulkan-Tutorial repo cloned')

    # install vcpkg
    try:
        subprocess.call(['vcpkg'])
    except FileNotFoundError:
        print('[] vcpkg not found. Installing now')

        repo_url = 'https://github.com/microsoft/vcpkg'
        install_dir = Path(os.path.abspath(os.path.dirname(sys.argv[0]))) / 'extern' / 'vcpkg'
        if not (install_dir / ".git").exists():
            print('Clonning vcpkg repo to : ', str(install_dir))
            subprocess.run(['git', 'clone', repo_url, str(install_dir)], check = True)

        bat_file = install_dir / "bootstrap-vcpkg.bat"
        subprocess.run(['cmd.exe', '/c', str(bat_file)], cwd = install_dir, check = True)

        print ('\n[] vcpkg installed')
    # end of vcpkg instalation

    # execution of the dependencies script
    print('[] Executing install_dependencies_windows.bat')

    script_path = Path(os.path.abspath(os.path.dirname(sys.argv[0]))) / 'extern' / 'Vulkan-Tutorial' / 'scripts' / 'install_dependencies_windows.bat'
    try:
        subprocess.run(['cmd.exe', '/c', str(script_path)], check = True)
    except FileNotFoundError:
        print('[] Couldnt run install_dependencies_windows.bat')

    print ('[] Dependencies installed succesfully')

    return

def install_vulkan_dependencies_linux():
    print('[] Installing linux dependencies')

    # clone the vulkan tutorial with the dependencies srcipt
    repo_url = 'https://github.com/KhronosGroup/Vulkan-Tutorial'
    install_dir = Path(os.path.abspath(os.path.dirname(sys.argv[0]))) / 'extern' / 'Vulkan-Tutorial'
    if not (install_dir / ".git").exists():
        print('[] Clonning Vulkan-Tutorial repo to : ', str(install_dir))
        subprocess.run(['git', 'clone', repo_url, str(install_dir)], check = True)

    print ('[] Vulkan-Tutorial repo cloned')

    # execution of the dependencies script
    print('[] Executing install_dependencies_linux.sh')

    script_path = Path(os.path.abspath(os.path.dirname(sys.argv[0]))) / 'extern' / 'Vulkan-Tutorial' / 'scripts' / 'install_dependencies_linux.sh'
    try:
        subprocess.run([script_path], check = True)
    except FileNotFoundError:
        print('[]Couldnt run install_dependencies_windows.sh')

    print ('[] Dependencies installed succesfully')


    return

# TODO #

def install_vulkan_dependencies_apple():
    return

def check_vulkan_sdk():

    if "VULKAN_SDK" in os.environ:
        print("[] Vulkan SDK Detected! skipping SDK instalation")
        return

    install_vulkan_sdk();
    return

def install_vulkan_sdk():
    print('[] Vulkan SDK not detected. Prepping instalation')

    system = check_os()

    # TODO #

    print('[] Vulkan SDK installed')
    return




if __name__ == "__main__" :

    install_vulkan_dependencies()
