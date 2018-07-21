#!/bin/bash
DRY_RUN=0

function execute {
  if [ ${DRY_RUN} -eq 1 ]
    then
    echo ">>Dry run. Command: $*"
  else
    eval $*
  fi
}

# calculate current VT
LVT=`fgconsole`

# calculate first usable display
XNUM="-1"
SOCK="something"
while [ ! -z "$SOCK" ] 
do
  XNUM=$(( $XNUM + 1 ))
  SOCK=$(ls -A -1 /tmp/.X11-unix | grep "X$XNUM" )
done

NEWDISP=":$XNUM"

if [ ! -z "$*" ] # generate exec line if arguments are given
then 
  # test if executable exists in path
  if [ -x "$(which $1 2> /dev/null)" ]
  then
    # generate exec line
    EXECL="$(which $1)"
  # test if executable exists on disk
  elif [ -e "$(realpath "$1")" ]
  then
    # generate exec line
    EXECL="$(realpath "$1")"
  else
    echo "$1: No such executable!"
    exit 1
  fi
  shift 1
  EXECL="$EXECL $*"
else # prepare to start new X sessions if no arguments passed
  EXECL=""
fi

#EXECL="/etc/X11/xinit/nvidia-xinitrc $EXECL"
COMMAND="xinit $EXECL -- $NEWDISP vt$LVT -nolisten tcp -br" # -config nvidia-xorg.conf -configdir nvidia-xorg.conf.d"

# --------- TURNING ON GPU -----------
echo 'Waking up nvidia GPU'
execute "sudo /root/gpu-ctl n"
sleep 1
execute "sudo /root/gpu-ctl s"

echo "Rescan  PCI devices"
execute "echo 1 | sudo tee /sys/bus/pci/rescan"
sleep 1
execute "sudo lspci | grep VGA"

# ---------- LOADING MODULES ----------
echo 'Loading nvidia module'
execute "sudo modprobe nvidia"

echo 'Loading nvidia_uvm module'
execute "sudo modprobe nvidia_uvm"

echo 'Loading nvidia_modeset module'
execute "sudo modprobe nvidia_modeset"

echo 'Loading nvidia_drm module'
execute "sudo modprobe nvidia_drm"

echo "Switch gmux to Nvidia GPU"
execute "sudo /root/gpu-ctl d"

# ---------- EXECUTING COMMAND --------
execute ${COMMAND}

# ---------- UNLOADING MODULES --------
echo 'Unloading nvidia_drm module'
execute "sudo rmmod nvidia_drm"

echo 'Unloading nvidia_modeset module'
execute "sudo rmmod nvidia_modeset"

echo 'Unloading nvidia_uvm module'
execute "sudo rmmod nvidia_uvm"

echo 'Unloading nvidia module'
execute "sudo rmmod nvidia"

# --------- TURNING OFF GPU ----------
echo 'Turning off nvidia GPU'
execute "echo 1 | sudo tee /sys/bus/pci/devices/0000:01:00.0/remove"
execute "sudo /root/gpu-ctl f"

echo "Switch gmux to Intel GPU"
execute "sudo /root/gpu-ctl i"


