import sys
import os
import glob

if len(sys.argv) <= 2:
    print(
        "Not enough arguments [param_RootPath, optional: param_SourceIdentifier]")
    exit()

param_RootPath = sys.argv[1].replace('\\', '/')
param_OutputPath = sys.argv[2].replace('\\', '/')

if len(sys.argv) == 4:
    param_SourceIdentifier = sys.argv[3]
else:
    param_SourceIdentifier = None


def get_files_recursive(path, filter, identifier):
    files = []
    for x in os.walk(path):
        for filePath in glob.glob(os.path.join(x[0], filter)):
            if identifier != None:
                firstLine = open(filePath).readline().rstrip()
                if firstLine.find(identifier) != -1:
                    files.append("    \"" + "${ROOT_DIR}/src" +
                                 filePath[len(path):].replace('\\', '/') + "\"")
            else:
                files.append("    \"" + "${ROOT_DIR}/src" +
                             filePath[len(path):].replace('\\', '/') + "\"")
    return files


# Get source files
source_files = []
source_files += get_files_recursive(param_RootPath,
                                    "*.c", param_SourceIdentifier)
source_files += get_files_recursive(param_RootPath,
                                    "*.cpp", param_SourceIdentifier)

# Get header files
header_files = []
header_files += get_files_recursive(param_RootPath,
                                    "*.h", None)
header_files += get_files_recursive(param_RootPath,
                                    "*.hpp", None)

source_output = "set(sources\n${SOURCES}\n)".replace(
    "${SOURCES}", "\n".join(source_files))
header_output = "set(headers\n${HEADERS}\n)".replace(
    "${HEADERS}", "\n".join(header_files))

with open(param_OutputPath, "w") as output_file:
    output_file.write(source_output + "\n\n" + header_output)
