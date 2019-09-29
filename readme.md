# Conditional Dockerfile initiative

This simple scripts allows the user to write conditional dockerfiles. This is pretty useful when a developer wants to create multiple containerized environments that have many things in common but differ in only a few installation steps. 

## What's the syntax? 

The syntax is pretty simple and also helps to improve the Dockerfile's documentation. It looks like this
```dockerfile
FROM python:latest
# NUMPY:
RUN pip3 install numpy
# ---
# PANDAS:
RUN pip3 install pandas
# ---
ENTRYPOINT python3
```

## How do I use it?

First compile the program.
```bash
make build
```

Then, write your configuration `images.yml` file.
This will be used to determine which images will be built and what their content will be.
For example:
```yml
numpy:
  - NUMPY
pandas:
  - PANDAS
full:
  - NUMPY
  - PANDAS
```
Now this build build a _numpy_ image with the NUMPY segment, a _pandas_ image with the PANDAS segment and a _full_ image with both.

Just have it run where your conditional dockerfile is with the following command.

```bash
./dockif_build
```

## Disclaimer
Super super super beta version. 

