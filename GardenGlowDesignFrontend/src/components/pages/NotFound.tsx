import React from 'react';
import {Box} from "@mui/material";

export const NotFound: React.FC = () => {
    return (
        <Box
            sx={{
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'center',
                justifyContent: 'center',
                height: '100vh',
                width: '100vw',
            }}
        >
            <img
                src="/back-away-slowly.jpg"
                alt="Back away slowly..."
                style={{
                    width: '20%',
                    height: 'auto'
                }}
            />
            <h1>Page Not Found</h1>
            <h3>Get outta here, you! Shoo! 🔫</h3>
        </Box>
    );
}