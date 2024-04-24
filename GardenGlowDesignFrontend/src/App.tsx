import React from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import HomePage from './components/pages/Home';
import LoginPage from './components/pages/Login';
import ConfigurationPage from './components/pages/Configuration';

import './App.css';

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<HomePage />} />
        <Route path="/login" element={<LoginPage />} />
        <Route path="/configuration" element={<ConfigurationPage />} />
      </Routes>
    </Router>
  );
}

export default App;
