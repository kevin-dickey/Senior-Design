import React, { useState } from 'react';
import Button from '@mui/material/Button';
import CssBaseline from '@mui/material/CssBaseline';
import TextField from '@mui/material/TextField';
import FormControlLabel from '@mui/material/FormControlLabel';
import Checkbox from '@mui/material/Checkbox';
import { Link as RouterLink } from 'react-router-dom';
import Grid from '@mui/material/Grid';
import Box from '@mui/material/Box';
import Typography from '@mui/material/Typography';
import Container from '@mui/material/Container';
import { createTheme, ThemeProvider } from '@mui/material/styles';
import axios from "axios";

const defaultTheme = createTheme();

export default function SignIn() {

  const [formData, setFormData] = useState({
    username: '',
    password: '',
  });

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();
    
    const data = new FormData(event.currentTarget);

    const username = data.get('username') as string;
    const password = data.get('password') as string;

    setFormData({username, password});

    try {
      const response = await axios.post('/api/signin', formData);
      console.log(response.data);
    } catch (error) {
      console.error('Error signing up:', error);
    }

  };

  return (
    <ThemeProvider theme={defaultTheme}>
      <Container component="main" maxWidth="xs">
        <CssBaseline />
        <Box
          sx={{
            marginTop: 8,
            display: 'flex',
            flexDirection: 'column',
            alignItems: 'center',
          }}
        >
          <Typography component="h1" variant="h5">
            Sign in
          </Typography>
          <Box component="form" onSubmit={handleSubmit} noValidate sx={{ mt: 1 }}>
            <TextField
              margin="normal"
              required
              fullWidth
              id="username"
              label="Username"
              name="username"
              autoComplete="username"
              autoFocus
            />
            <TextField
              margin="normal"
              required
              fullWidth
              name="password"
              label="Password"
              type="password"
              id="password"
              autoComplete="current-password"
            />
            <FormControlLabel
              control={<Checkbox value="remember" color="primary" />}
              label="Remember me"
            />
            <Button
            type="submit"
              component={RouterLink} 
              to="/"
              fullWidth
              variant="contained"
              sx={{ mt: 3, mb: 2 }}
            >
              Sign In
            </Button>
            <Grid container>
              <Grid item xs>
                {/* <Typography
                  variant="body2"
                  //to do: add link
                  // component={RouterLink}
                  // to="/"
                  color="primary"
                  style={{ textDecoration: 'none' }}
                >
                  Forgot password?
                </Typography> */}
              </Grid>
              <Grid item>
                {/* <Typography
                  variant="body2"
                  component={RouterLink}
                  to="/signup"
                  color="primary"
                  style={{ textDecoration: 'none' }}
                >
                  {"Don't have an account? Sign Up"}
                </Typography> */}
              </Grid>
            </Grid>
          </Box>
        </Box>
      </Container>
    </ThemeProvider>
  );
}