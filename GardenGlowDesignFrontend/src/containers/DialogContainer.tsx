import * as React from 'react';
import * as yup from 'yup';
import {Field, Form, Formik} from "formik";

import {ThemeProvider} from "@mui/material/styles";
import CssBaseline from "@mui/material/CssBaseline";
import darkTheme from "../utils/Theming";

import Button from '@mui/material/Button';
import TextField from "@mui/material/TextField";

import Dialog from '@mui/material/Dialog';
import DialogActions from '@mui/material/DialogActions';
import DialogContent from '@mui/material/DialogContent';
import DialogContentText from '@mui/material/DialogContentText';
import DialogTitle from '@mui/material/DialogTitle';

import {Show} from "../components/serialization/Show";
import {GridLayout} from "../components/serialization/Layout";

export interface DialogContainerProps {
    show: Show | null,
    open: boolean,
    onClose: () => void
    onSubmit: (formValues: any) => void,
}

export const DialogContainer: React.FC<DialogContainerProps> = (props) => {
    const grid = props.show?.layouts[0] as GridLayout;

    const initialValues = props.show ? {
        showName: props.show.name,
        height: grid.height,
        width: grid.width,
        } : {
        showName: '',
        height: 0,
        width: 0
    };

    const validationSchema = yup.object({
        showName: yup
            .string()
            .label("Show Name")
            .required(),
        height: yup
            .number()
            .label("Field Height")
            .required()
            .positive(),
        width: yup
            .number()
            .label("Field Width")
            .required()
            .positive(),
    });

    return (
        <ThemeProvider theme={darkTheme}>
            <CssBaseline/>
            <React.Fragment>
                <Dialog
                    open={props.open}
                    onClose={props.onClose}
                >
                    <DialogTitle>Show Configuration</DialogTitle>
                    <Formik
                        initialValues={initialValues}
                        validationSchema={validationSchema}
                        onSubmit={(values) => {props.onSubmit(values)}}
                    >
                        {({errors, touched}) => (
                            <Form>
                                <DialogContent>
                                    <DialogContentText>
                                        Update the show attributes here!
                                    </DialogContentText>
                                    <Field
                                        as={TextField}
                                        id="showName"
                                        name="showName"
                                        label="Show Name"
                                        fullWidth
                                        margin="dense"
                                        error={touched.showName && Boolean(errors.showName)}
                                        helperText={touched.showName && errors.showName}
                                    />
                                    <Field
                                        as={TextField}
                                        id="height"
                                        name="height"
                                        label="Height (Divide by 2)"
                                        type="number"
                                        fullWidth
                                        margin="dense"
                                        error={touched.height && Boolean(errors.height)}
                                        helperText={touched.height && errors.height}
                                    />
                                    <Field
                                        as={TextField}
                                        id="width"
                                        name="width"
                                        label="Width (Divide by 2)"
                                        type="number"
                                        fullWidth
                                        margin="dense"
                                        error={touched.width && Boolean(errors.width)}
                                        helperText={touched.width && errors.width}
                                    />
                                </DialogContent>
                                <DialogActions>
                                    <Button onClick={props.onClose}>Cancel</Button>
                                    <Button type="submit">Submit</Button>
                                </DialogActions>
                            </Form>
                        )}
                    </Formik>
                </Dialog>
            </React.Fragment>
        </ThemeProvider>
    )
        ;
}
