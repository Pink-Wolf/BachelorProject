import BaseEntityViewer from "./BaseEntityViewer";
import ClassViewer from "./ClassViewer";
import FunctionViewer from "./FunctionViewer";
import VariableViewer from "./VariableViewer";

export default function EntityViewer(props) {
	const data = props.data;
	switch (data?.type) {
		case `class`: return <ClassViewer data={data} />
		case `function`: return <FunctionViewer data={data} />
		case `variable`: return <VariableViewer data={data} />
		default: return <BaseEntityViewer />
    }
}