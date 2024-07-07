#!/bin/bash

# Create a virtual environment
python3 -m venv venv

# Activate the virtual environment
source venv/bin/activate

# Install Flask
pip install Flask

# Install bson
pip install bson

# Install pymongo
pip install pymongo


# Run the Flask app
export FLASK_APP=app.py
flask run --host=0.0.0.0