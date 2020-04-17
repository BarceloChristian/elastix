Install dependencies from APT
```bash
sudo apt-get update && sudo apt-get install -y libinsighttoolkit4-dev libann-dev
```

Then, run `package.sh` with sudo privileges (We may change this in the future)
```bash
sudo ./package.sh
```

Congratulations! The debian package has been generated, go ahead and use dpkg to install it.