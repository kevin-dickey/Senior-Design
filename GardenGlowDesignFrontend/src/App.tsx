import React from 'react';
import {BrowserRouter as Router, Route, Routes} from 'react-router-dom';
import HomePage from './components/pages/Home';
import './App.css';
import {ConfigurationContainer} from "./containers/ConfigurationContainer";
import LoginContainer from './containers/LoginContainer';
import OverviewContainer from './containers/OverviewContainer';
import LEDGrid from './components/pages/Grid';
import {NotFound} from "./components/pages/NotFound";


function App() {
    return (
        <Router>
            <Routes>
                <Route path="*" element={<NotFound/>}/>
                <Route path="/" element={<HomePage/>}/>
                <Route path="/login" element={<LoginContainer/>}/>
                <Route path="/configuration" element={<ConfigurationContainer />}/>
                <Route path="/shows/*" element={<OverviewContainer/>}/>
                <Route path="/grid" element={<LEDGrid/>}/>
            </Routes>
        </Router>
    );
}

export default App;
