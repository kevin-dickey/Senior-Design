import * as React from 'react';
import AppBar from '@mui/material/AppBar';
import Box from '@mui/material/Box';
import Toolbar from '@mui/material/Toolbar';
import IconButton from '@mui/material/IconButton';
import ArrowBackRoundedIcon from '@mui/icons-material/ArrowBackRounded';
import SaveRoundedIcon from '@mui/icons-material/SaveRounded';
import SettingsRoundedIcon from '@mui/icons-material/SettingsRounded';
import Typography from '@mui/material/Typography';
import Menu from '@mui/material/Menu';
import Container from '@mui/material/Container';
import Avatar from '@mui/material/Avatar';
import Tooltip from '@mui/material/Tooltip';
import MenuItem from '@mui/material/MenuItem';
import AdbIcon from '@mui/icons-material/Adb';
import {ClassNameMap} from "@mui/styles";
import {Chip, Popover} from "@mui/material";

export interface NavBarProps {
    classes?: (Partial<ClassNameMap<never>>) | undefined;
    showName: string;
    onClickSettings: () => void;
    onClickAccount: () => void;
    onClickHome: () => void;
    onClickSave: () => void;
    onClickExport: () => void;
}

const settings = ['Profile', 'Account', 'Dashboard', 'Logout'];


const NavBar: React.FC<NavBarProps> = (props) => {
    const [anchorElUser, setAnchorElUser] = React.useState<null | HTMLElement>(null);
    const [anchorElSave, setAnchorElSave] = React.useState<null | HTMLElement>(null);
    const open = Boolean(anchorElSave);

    const handleOpenUserMenu = (event: React.MouseEvent<HTMLElement>) => {
        setAnchorElUser(event.currentTarget);
    };

    const handleCloseUserMenu = () => {
        setAnchorElUser(null);
    };

    const handleSavePopoverOpen = (event: React.MouseEvent<HTMLElement>) => {
        setAnchorElSave(event.currentTarget);
    }

    const handleSavePopoverClose = () => {
        setAnchorElSave(null);
    }

    const handleSaveClick = () => {
        if (open) {
            props.onClickSave();
        }
    }

    return (
        <AppBar position="static" sx={{zIndex: (theme) => theme.zIndex.drawer + 1}}>
            <Container maxWidth={false}>
                <Toolbar disableGutters>
                    <Box sx={{flexGrow: 1, display: 'flex'}}>
                        <IconButton
                            size="large"
                            edge="start"
                            color="inherit"
                            aria-label="back"
                            onClick={props.onClickHome}
                        >
                            <ArrowBackRoundedIcon/>
                        </IconButton>
                        <Box>
                            <Typography sx={{my: 'auto'}}>
                                {props.showName}
                            </Typography>
                            <Chip
                                color='primary'
                                label='Unsaved Changes'
                                variant='filled'
                                sx={{
                                    borderRadius: '5px',
                                    height: 'min-content'
                                }}
                            />
                        </Box>
                    </Box>
                    <AdbIcon sx={{display: 'flex', mr: 1}}/>
                    <Typography
                        variant="h6"
                        noWrap
                        component="a"
                        sx={{
                            mr: 2,
                            display: 'flex',
                            flexGrow: 1,
                            fontFamily: 'monospace',
                            fontWeight: 700,
                            letterSpacing: '.3rem',
                            color: 'inherit',
                            textDecoration: 'none',
                        }}
                    >
                        Garden Glow
                    </Typography>

                    <Box
                        sx={{
                            flexGrow: 0
                        }}
                        onMouseEnter={handleSavePopoverOpen}
                        onMouseLeave={handleSavePopoverClose}
                        onClick={handleSaveClick}
                    >
                        <IconButton
                            size="large"
                            edge="start"
                            color="inherit"
                            aria-label="Save Show"
                        >
                            <SaveRoundedIcon/>
                        </IconButton>
                        <Popover
                            open={open}
                            anchorEl={anchorElSave}
                            onClose={handleSavePopoverClose}
                            anchorOrigin={{
                                vertical: 'bottom',
                                horizontal: 'left',
                            }}
                        >
                            <MenuItem
                                onClick={() => {
                                    props.onClickSave();
                                    handleSavePopoverClose();
                                }}>
                                Save
                            </MenuItem>
                            <MenuItem
                                onClick={() => {
                                    props.onClickExport();
                                    handleSavePopoverClose();
                                }}>
                                Export
                            </MenuItem>
                        </Popover>
                        <IconButton
                            size="large"
                            edge="start"
                            color="inherit"
                            aria-label="Show Settings"
                            onClick={props.onClickSettings}
                        >
                            <SettingsRoundedIcon/>
                        </IconButton>
                        <Tooltip title="Open settings">
                            <IconButton onClick={handleOpenUserMenu} sx={{p: 0}}>
                                <Avatar
                                    alt="Remy Sharp"
                                    src="/static/images/avatar/2.jpg"
                                    sx={{width: 32, height: 32}}
                                />
                            </IconButton>
                        </Tooltip>
                        <Menu
                            sx={{mt: '45px'}}
                            id="menu-appbar"
                            anchorEl={anchorElUser}
                            anchorOrigin={{
                                vertical: 'top',
                                horizontal: 'right',
                            }}
                            keepMounted
                            transformOrigin={{
                                vertical: 'top',
                                horizontal: 'right',
                            }}
                            open={Boolean(anchorElUser)}
                            onClose={handleCloseUserMenu}
                        >
                            {settings.map((setting) => (
                                <MenuItem key={setting} onClick={handleCloseUserMenu}>
                                    <Typography sx={{textAlign: 'center'}}>{setting}</Typography>
                                </MenuItem>
                            ))}
                        </Menu>
                    </Box>
                </Toolbar>
            </Container>
        </AppBar>
    );
}
export default NavBar;