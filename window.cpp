#include "window.h"
#include "adcreader.h"

//#include <cmath>  // for sine stuff
//double max=0.0;

Window::Window() : func(0),count(0)
{
	
	knob = new QwtKnob;
	// set up the gain knob 
	knob->setValue(0);

	// use the Qt signals/slots framework to update the gain -
	// every time the knob is moved, the setFunc function will be called
	connect( knob, SIGNAL(valueChanged(double)), SLOT(setFunc(double)) );

	// set up the thermometer
        //thr=new QPushButton;
	//thr->setText(tr("Threshold"));
	//thr->show();
//        m_label = new QwtTextLabel;
//	m_label->setText("Max: ");
//	m_label->show();
//	m1_label= new QwtTextLabel;
//	m1_label->setText("0");
//	m1_label->show();

	// set up the initial plot data
	for( int index=0; index<plotDataSize; ++index )
	{
		xData[index] = index;
		yData[index] = 0;
	}

	curve = new QwtPlotCurve;
	plot = new QwtPlot;
	// make a plot curve from the data and attach it to the plot
	curve->setSamples(xData, yData, plotDataSize);
	curve->attach(plot);

	plot->replot();
	plot->show();


	// set up the layout - knob above thermometer
	vLayout = new QVBoxLayout;
	vLayout->addWidget(knob);
//	vLayout->addWidget(m_label);
//	vLayout->addWidget(m1_label);
//	vLayout->addWidget(thermo);

	// plot to the left of knob and thermometer
	hLayout = new QHBoxLayout;
	hLayout->addLayout(vLayout);
	hLayout->addWidget(plot);

	setLayout(hLayout);

	// This is a demo for a thread which can be
	// used to read from the ADC asynchronously.
	// At the moment it doesn't do anything else than
	// running in an endless loop and which prints out "tick"
	// every second.
	adcreader = new ADCreader();
	adcreader->start();
}

Window::~Window() {
	// tells the thread to no longer run its endless loop
	adcreader->quit();
	// wait until the run method has terminated
	adcreader->wait();
//	delete adcreader;
}

void Window::timerEvent( QTimerEvent * )
{
	int inval;
	double value;
	while(adcreader->read_enable()){
				
		inval=adcreader->get_samples();
		value=(double) inval;
		fprintf(stderr,"int = %d  double = %f     \r",inval,value);

		
		// add the new input to the plot
		memmove( yData, yData+1, (plotDataSize-1) * sizeof(double) );
		yData[plotDataSize-1] = value;
		curve->setSamples(xData, yData, plotDataSize);
		plot->replot();
	}
	// set the thermometer value
	//thermo->setValue( inVal + 20 );
}


// this function can be used to change the gain of the A/D internal amplifier
void Window::setFunc(double func)
{
	// for example purposes just change the amplitude of the generated input
	this->func = func;
}
