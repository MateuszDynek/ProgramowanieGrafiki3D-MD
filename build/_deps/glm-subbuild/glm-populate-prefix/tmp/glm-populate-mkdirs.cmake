# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-src"
  "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-build"
  "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-subbuild/glm-populate-prefix"
  "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-subbuild/glm-populate-prefix/tmp"
  "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
  "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-subbuild/glm-populate-prefix/src"
  "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/m.dynek/Documents/Repo/Programowanie3D/3d/build/_deps/glm-subbuild/glm-populate-prefix/src/glm-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
