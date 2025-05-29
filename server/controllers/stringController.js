const express = require('express');
const router = express.Router();

// Function to handle the POST request for /send-string
const handleSendString = (req, res) => {
  const receivedString = req.body.string;

  if (!receivedString) {
    return res.status(400).json({ error: 'No string provided' });
  }

  console.log('Received string from ESP32:', receivedString);

  // Respond with a success message
  res.json({ message: 'String received successfully', receivedString });
};

module.exports = { handleSendString };