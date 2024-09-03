import { Container, Typography, Box, Button, ThemeProvider, createTheme } from '@mui/material';
import React from "react";
import { Link } from 'react-router-dom';
import CssBaseline from '@mui/material/CssBaseline';

const darkTheme = createTheme({
  palette: {
    mode: 'dark',
  },
});

const HomePage : React.FC = () => {
  return (
    <ThemeProvider theme={darkTheme}>
    <Container component="main">
      <CssBaseline />

    <Box
      sx={{
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        height: '100vh',
        backgroundImage: 'url(/path/to/your/background/image.jpg)',
        backgroundSize: 'cover',
        textAlign: 'center',
      }}
    >
      <Typography variant="h2" component="h1" gutterBottom>
        Welcome to the Interactive Lighting Exhibit
      </Typography>
      <Typography variant="h5" component="h2" gutterBottom>
        Experience the future of interactive art
      </Typography>
      <Box sx={{ mt: 4 }}>
        <Button
          variant="contained"
          component={Link}
          to="/login"
          sx={{ mr: 2 }}
        >
          Login
        </Button>
        <Button
          component={Link}
          to="https://sddec24-15.sd.ece.iastate.edu/"
        >
          Learn More
        </Button>
      </Box>
    </Box>
  </Container>
  </ThemeProvider>
);
};

export default HomePage;