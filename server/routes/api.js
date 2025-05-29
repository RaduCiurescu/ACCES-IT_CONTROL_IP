const express = require('express');
const router = express.Router();
const { handleSendString } = require('../controllers/stringController');

// POST endpoint to receive the string
router.post('/send-string', handleSendString);

module.exports = router;