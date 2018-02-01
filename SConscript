###############################################################################
# Copyright 2017 Samsung Electronics All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
###############################################################################

################ ezmq_plus build script ##################

import os

Import('env')

ezmq_plus_env = env.Clone()
target_os = ezmq_plus_env.get('TARGET_OS')
target_arch = ezmq_plus_env.get('TARGET_ARCH')

if ezmq_plus_env.get('RELEASE'):
    ezmq_plus_env.AppendUnique(CCFLAGS=['-Os'])
else:
    ezmq_plus_env.AppendUnique(CCFLAGS=['-g'])

ezmq_plus_env.AppendUnique(CPPPATH=[
        './dependencies/protocol-ezmq-cpp/include',
        './dependencies/protocol-ezmq-cpp/extlibs/zmq',
        './dependencies/protocol-ezmq-cpp/protobuf',
#         './extlibs/zmq',
#        './protobuf',
        './include',
#        './include/logger',
        './internal'
])

ezmq_plus_env.PrependUnique(LIBS=['zmq', 'protobuf'])

if ezmq_plus_env.get('RELEASE'):
    ezmq_plus_env.PrependUnique(LIBS=['ezmq'], LIBPATH=[os.path.join('./dependencies/protocol-ezmq-cpp/out/linux/', target_arch, 'release')])
else:
    ezmq_plus_env.PrependUnique(LIBS=['ezmq'], LIBPATH=[os.path.join('./dependencies/protocol-ezmq-cpp/out/linux/', target_arch, 'debug')])

#ezmq_plus_env.PrependUnique(LIBS=['protobuf'])


if target_os not in ['windows']:
    ezmq_plus_env.AppendUnique(
        CXXFLAGS=['-O2', '-g', '-Wall', '-fPIC', '-fmessage-length=0', '-std=c++11', '-I/usr/local/include'])

if target_os not in ['windows']:
    ezmq_plus_env.AppendUnique(LINKFLAGS=['-Wl,--no-undefined'])

if target_os in ['linux']:
    ezmq_plus_env.AppendUnique(LIBS=['pthread'])

#if target_os in ['linux']:
#    if not env.get('RELEASE'):
#        ezmq_plus_env.PrependUnique(LIBS=['gcov'])
#        ezmq_plus_env.AppendUnique(CXXFLAGS=['--coverage'])

ezmq_plus_DIR = '.'
ezmq_plus_env.AppendUnique(ezmq_plus_src = [ezmq_plus_env.Glob(os.path.join(ezmq_plus_DIR, 'src', '*.cpp'))])
#                                ezmq_plus_env.Glob(os.path.join(ezmq_plus_DIR, 'src', 'logger', '*.cpp')),
#                                ezmq_plus_env.Glob(os.path.join(ezmq_plus_DIR, 'protobuf', '*.cc'))])

ezmq_plus_shared = ezmq_plus_env.SharedLibrary('ezmq_plus', ezmq_plus_env.get('ezmq_plus_src'))
ezmq_plus_static = ezmq_plus_env.StaticLibrary('ezmq_plus', ezmq_plus_env.get('ezmq_plus_src'))

# Go to build ezmq_plus sample apps
#if target_os == 'linux':
#       SConscript('samples/SConscript')

# Go to build ezmq_plus unit test cases
#if target_os == 'linux':
#    if target_arch in ['x86', 'x86_64']:
#        SConscript('unittests/SConscript')
