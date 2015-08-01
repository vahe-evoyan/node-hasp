# NodeJS wrapper for Sentinel HASP API

## Build Instructions

### Linux (Ubuntu 14.04.02 LTS Trusty Tahr)

Let's confirm the OS version, as other ones are not tested.

```
$ lsb_release -a
No LSB modules are available.
Distributor ID: Ubuntu
Description:  Ubuntu 14.04.2 LTS
Release:  14.04
Codename: trusty
```

We consider a plain OS system not having installed previously, so let's update
the system to have latest packages.

```
$ sudo apt-get update && sudo apt-get -y upgrade
```

Install build tools and Node JS v0.12

```
$ sudo apt-get install build-essential
$ curl -sL https://deb.nodesource.com/setup_0.12 | sudo bash -
$ sudo apt-get install -y nodejs
```

Upgrade to the latest npm version

```
$ sudo npm install -g npm
```

Install Git and clone the repository.

```
$ sudo apt-get install git
$ git clone https://github.com/vahe-evoyan/node-hasp.git
```

Enter the directory and install the dependencies

```
$ cd node-hasp
$ npm install
```

In case if you get an EACCESS error, you need to change the ownership of your
`~/.npm` directory.

```
$ sudo chown -R <my user>:<my group> ~/.npm
```

Finally build and test the module by running `npm test`. Note that the demo key
should be attached in order to run the default tests.
