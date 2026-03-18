# Weightloss-Tracker

Arduino project for tracking weight loss progress with LEDs

## Current State

> **NOTE:**
> The current state is only for testing connecting to the arduino wirelessly via bluetooth.

Upload the controller code (led-controller.ino) to the arduino and visit the web controller in order to connect to and control the arduino.

## Led Controller (led-controller.ino)

Contains logic for controlling the LEDs connected to the arduino based on the weight variables.

## Web Controller (web-controller.ino)

Web app that handles sending values to the arduino over bluetooth.
