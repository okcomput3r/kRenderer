import os;
import pprint;

def check_vulkan_sdk():

    if "VULKAN_SDK" in os.environ:
        return
    install_vulkan_sdk();

def install_vulkan_sdk():
    return

pp = pprint.PrettyPrinter(width=41, compact=True)


pp.pprint('Checking for dependencies')
pp.pprint(os.name)
