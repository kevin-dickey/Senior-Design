import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import Login from '../components/pages/Login';

const credentials = {
  username: 'username',
  password: 'password',
};

const LoginContainer: React.FC = () => {
  const [formData, setFormData] = useState({
    username: '',
    password: '',
  });
  const [error, setError] = useState('');
  const navigate = useNavigate();

  const handleSubmit = async (event: React.FormEvent<HTMLFormElement>) => {
    event.preventDefault();

    const data = new FormData(event.currentTarget);
    const username = data.get('username') as string;
    const password = data.get('password') as string;

    setFormData({ username, password });

    if (username === credentials.username && password === credentials.password) {
        navigate('/foldersoverview');
    } else {
      setError('Incorrect username or password');
    }
  };

  return <Login handleSubmit={handleSubmit} error={error} />;
};

export default LoginContainer;
