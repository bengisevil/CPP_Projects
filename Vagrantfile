$bootstrap = <<BOOTSTRAP
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get -y install build-essential
apt-get -y install cmake valgrind lcov
apt-get -y install python3-pip
pip3 install gcovr
pip3 install pexpect

# install graphical environment
apt-get -y install xorg
apt-get -y install nodm openbox
apt-get -y install virtualbox-guest-dkms 
apt-get -y install virtualbox-guest-utils 
apt-get -y install virtualbox-guest-x11

# install Qt
apt-get -y install qt5-default

# edit nodm config
sed -ie 's|NODM_ENABLED=false|NODM_ENABLED=true|g' /etc/default/nodm
sed -ie 's|NODM_USER=root|NODM_USER=ubuntu|g' /etc/default/nodm
sed -ie 's|NODM_XSESSION=/etc/X11/Xsession|NODM_XSESSION=/home/ubuntu/.xsession|g' /etc/default/nodm

# create .xsession 
echo "openbox-session" > /home/ubuntu/.xsession

# make executable
chmod +x /home/ubuntu/.xsession

# set the default display
echo "export DISPLAY=:0" >> /home/ubuntu/.bashrc

echo "sudo VBoxClient --clipboard" >> /home/ubuntu/.bashrc
echo "sudo VBoxClient --draganddrop" >> /home/ubuntu/.bashrc
echo "sudo VBoxClient --display" >> /home/ubuntu/.bashrc
echo "sudo VBoxClient --checkhostversion" >> /home/ubuntu/.bashrc
echo "sudo VBoxClient --seamless" >> /home/ubuntu/.bashrc
BOOTSTRAP

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/xenial64"

  # get rig of annoying console.log file 
  config.vm.provider "virtualbox" do |vb|
    vb.customize [ "modifyvm", :id, "--uartmode1", "disconnected" ]
    vb.gui = true
  end

  # setup the VM
  config.vm.provision "shell", inline: $bootstrap
end
