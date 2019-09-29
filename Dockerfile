FROM python:latest
# NUMPY:
RUN pip install numpy 
# ---
# PANDAS:
RUN pip install pandas
# ---
ENTRYPOINT python